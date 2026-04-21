from pathlib import Path

Import("env")


def append_if_exists(path: Path) -> bool:
    if path.exists():
        env.Append(CPPPATH=[str(path)])
        print(f"Added include path: {path}")
        return True
    return False


project_include_dir = Path(env.subst("$PROJECT_INCLUDE_DIR"))
append_if_exists(project_include_dir)

framework_dir = env.PioPlatform().get_package_dir("framework-arduinoespressif32")
if framework_dir:
    append_if_exists(Path(framework_dir) / "libraries" / "Network" / "src")