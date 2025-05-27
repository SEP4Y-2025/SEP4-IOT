import os
import subprocess
from pathlib import Path


print("=== Running generate_coverage.py ===")

build_dir = Path("coverage")
coverage_dir = Path("coverage")
coverage_dir.mkdir(exist_ok=True)

# Run gcovr to generate HTML coverage report
try:
    subprocess.run([
        "gcovr",
        "-r", ".", 
        "--html", "--html-details", 
        f"--output={coverage_dir}/coverage.html",
        "--exclude-directories", "test", 
        "--exclude-directories", "lib"
    ], check=True)

    print(f"✔ Coverage report generated at {coverage_dir}/coverage.html")
except subprocess.CalledProcessError as e:
    print("✖ Failed to generate coverage:", e)
