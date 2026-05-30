#!/usr/bin/env python3
import tarfile
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
ARCHIVES_DIR = ROOT / "lib"

ARCHIVES = {
    "capureclt_libs.tar.gz": ROOT / "capureclt",
    "capureser_libs.tar.gz": ROOT / "capureser",
}

def decompress():
    for archive_name, target_dir in ARCHIVES.items():
        archive_path = ARCHIVES_DIR / archive_name
        if not archive_path.exists():
            print(f"{archive_name} not found, skipping")
            continue

        # Remove existing lib/ in target if present
        lib_dir = target_dir / "lib"
        if lib_dir.exists():
            print(f"Removing existing {lib_dir}")
            shutil.rmtree(lib_dir)

        print(f"Extracting {archive_name} → {target_dir}")
        with tarfile.open(archive_path, "r:gz") as tar:
            tar.extractall(path=target_dir)

        print(f"{archive_name} done → {target_dir}/lib/")

    print("\nAll libraries decompressed and placed correctly!\nnow you can build projects")

if __name__ == "__main__":
    decompress()
