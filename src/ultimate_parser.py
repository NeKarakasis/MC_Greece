from elftools.elf.elffile import ELFFile
from elftools.dwarf.descriptions import describe_form_class
import sys

# ============================
# INPUT ARGUMENTS
# ============================
if len(sys.argv) < 3:
    print("[ERROR] Usage: python elf_parser.py <elf_file> <output_file>")
    sys.exit(1)

ELF_FILE = sys.argv[1]
OUTPUT_FILE = sys.argv[2]

print(f"[INFO] ELF input : {ELF_FILE}")
print(f"[INFO] Output file: {OUTPUT_FILE}")


# ============================
# DEBUG LOGGING
# ============================
def log(msg):
    print(f"[INFO] {msg}")

def warn(msg):
    print(f"[WARN] {msg}")

def err(msg):
    print(f"[ERROR] {msg}")


# ============================
# TYPE HELPERS
# ============================
def get_type_name(die):
    if not die:
        return "unknown"
    if 'DW_AT_name' in die.attributes:
        return die.attributes['DW_AT_name'].value.decode(errors='ignore')
    return "unknown"


def map_type(name):
    mapping = {
        "int": "signed int",
        "unsigned int": "unsigned int",
        "short unsigned int": "unsigned short",
        "short int": "short",
        "char": "char",
        "bool": "bool",
    }
    return mapping.get(name, name)


# ============================
# OFFSET PARSER
# ============================
def get_offset(attr):
    val = attr.value

    if isinstance(val, int):
        return val

    if isinstance(val, list) and len(val) >= 2:
        try:
            return int(val[1])
        except:
            warn(f"Failed to decode offset list: {val}")
            return 0

    warn(f"Unknown offset format: {val}")
    return 0


# ============================
# TYPE RESOLUTION
# ============================
def resolve_type(die):
    if not die:
        return None

    if die.tag == "DW_TAG_base_type":
        name = map_type(get_type_name(die))
        size = die.attributes.get("DW_AT_byte_size", {}).value \
            if 'DW_AT_byte_size' in die.attributes else 4
        return {"kind": "base", "name": name, "size": size}

    if die.tag == "DW_TAG_pointer_type":
        return {"kind": "pointer", "name": "pointer", "size": 4}

    if die.tag == "DW_TAG_array_type":
        elem_die = die.get_DIE_from_attribute('DW_AT_type') \
            if 'DW_AT_type' in die.attributes else None
        elem_type = resolve_type(elem_die)

        count = 1
        for child in die.iter_children():
            if child.tag == "DW_TAG_subrange_type":
                if 'DW_AT_upper_bound' in child.attributes:
                    count = child.attributes['DW_AT_upper_bound'].value + 1
                elif 'DW_AT_count' in child.attributes:
                    count = child.attributes['DW_AT_count'].value

        if not elem_type:
            return None

        return {
            "kind": "array",
            "name": elem_type.get("name", "unknown"),
            "size": elem_type.get("size", 0) * count
        }

    if die.tag == "DW_TAG_structure_type":
        struct = {
            "kind": "struct",
            "size": die.attributes.get("DW_AT_byte_size", {}).value \
                if 'DW_AT_byte_size' in die.attributes else 0,
            "members": []
        }

        for child in die.iter_children():
            if child.tag == "DW_TAG_member" and 'DW_AT_name' in child.attributes:

                name = child.attributes['DW_AT_name'].value.decode(errors='ignore')
                type_die = child.get_DIE_from_attribute('DW_AT_type') \
                    if 'DW_AT_type' in child.attributes else None

                offset = 0
                if 'DW_AT_data_member_location' in child.attributes:
                    offset = get_offset(child.attributes['DW_AT_data_member_location'])

                struct["members"].append({
                    "name": name,
                    "type": type_die,
                    "offset": offset
                })

        return struct

    if 'DW_AT_type' in die.attributes:
        return resolve_type(die.get_DIE_from_attribute('DW_AT_type'))

    return None


# ============================
# EXPAND VARIABLES
# ============================
def expand(name, addr, type_info, f, is_global=True):

    if not type_info:
        warn(f"Skipping {name}: no type info")
        return

    if type_info["kind"] != "struct":
        size = type_info.get('size', 4)
        typename = type_info.get('name', 'unknown')

        if typename == "bool":
            typename = "_Bool"

        if type_info["kind"] == "pointer":
            typename = "[pointer]"

        if is_global:
            typename = "data ,g"

        f.write(f"{name}\n{addr:08x}  {size}  {typename}\n")
        return

    f.write(f"{name}\n{addr:08x}  {type_info['size']}  data ,g\nstruct  {{\n")

    for m in type_info["members"]:
        if not m["name"]:
            continue

        sub_name = f"{name}.{m['name']}"
        sub_addr = addr + m["offset"]

        sub_type = resolve_type(m["type"])
        expand(sub_name, sub_addr, sub_type, f, is_global=False)

    f.write("}\n")


# ============================
# MAIN
# ============================
def main():
    log("Opening ELF file...")

    try:
        with open(ELF_FILE, "rb") as f:

            elf = ELFFile(f)

            if not elf.has_dwarf_info():
                err("No DWARF info found in ELF!")
                return

            dwarf = elf.get_dwarf_info()
            log("DWARF info loaded successfully")

            var_count = 0

            with open(OUTPUT_FILE, "w") as out:
                out.write("Renesas Optimizing Linker\n")

                for cu in dwarf.iter_CUs():
                    for die in cu.iter_DIEs():

                        if die.tag != "DW_TAG_variable":
                            continue

                        if 'DW_AT_name' not in die.attributes:
                            continue

                        name = "_" + die.attributes['DW_AT_name'].value.decode(errors='ignore')

                        if 'DW_AT_location' not in die.attributes:
                            continue

                        loc = die.attributes['DW_AT_location']

                        if describe_form_class(loc.form) != 'exprloc':
                            continue

                        expr = loc.value

                        if len(expr) < 5 or expr[0] != 0x03:
                            continue

                        addr = int.from_bytes(expr[1:5], byteorder='little')

                        # ? INCLUDE BOTH RAM AND FLASH
                        if not (0x00000000 <= addr < 0x40000000):
                            continue

                        if 'DW_AT_type' not in die.attributes:
                            continue

                        type_die = die.get_DIE_from_attribute('DW_AT_type')
                        type_info = resolve_type(type_die)

                        expand(name, addr, type_info, out, is_global=True)
                        var_count += 1

            log("Parsing completed successfully")
            log(f"Variables parsed: {var_count}")
            log(f"Output file: {OUTPUT_FILE}")

    except Exception as e:
        err(f"Execution failed: {e}")


if __name__ == "__main__":
    main()