# CLAUDE.md — YAS Apt

## What
Native GUI wrapper for **APT** (`apt`). Part of YAS suite.
Status: **scaffolded & unit-tested** — vendored core + adapter + QML shell compile, 3/3 QtTest suites pass (verified cross-compiling on macOS). Pending: build + QA on the real target platform.

## Stack
- C++20 + Qt 6.7+ (Qt Quick / QML), CMake ≥ 3.24, GCC/Clang
- Native windowing via Qt QPA plugins: **wayland** with **xcb** (X11) fallback. Qt picks at runtime; test both.
- CLI execution: `QProcess` wrapping `apt` / `apt-get` / `apt-cache`. Never bundle it.
- Architecture: **vendored core copy** (identical across suite, NO shared library by design) + `apt` adapter. Master template: `../yas-core/` local folder (not published). Core fixes must be replicated across repos.

## Target platform
Debian / Ubuntu and derivatives. x64 + arm64.

## apt specifics
- **install/remove/upgrade/update require root.** Use **polkit** (`pkexec`) for privileged operations — never run the whole GUI as root, never store passwords. Ship a polkit policy file. This is the hardest platform problem — design it first.
- `apt` CLI warns "unstable CLI interface" for scripting: use `apt-get`/`apt-cache`/`dpkg-query` for machine parsing, `apt` only for human-facing log view.
- Key commands: `apt-cache search/show`, `dpkg-query -W -f`, `apt-get install/remove/purge/autoremove -y`, `apt-get update`, `apt-get upgrade`, `apt-mark hold/unhold/showhold`, `apt-get clean/autoclean`, `apt list --upgradable`.
- Interactive prompts: always use `-y` + `DEBIAN_FRONTEND=noninteractive` where safe; surface conffile conflicts in UI.
- dpkg lock (`/var/lib/dpkg/lock-frontend`): detect and queue operations; another apt process blocks everything.
- Consider libapt-pkg bindings later; v1 wraps CLI per suite convention.

## Design (see DESIGN.md)
- Dark theme. Base `#212826`, accent **Red `#D32F2F`**, highlight `#D32F2F1A`, text `#F8F8F2` / `#ACADAD`.
- App tag: **APT**. Fonts: Outfit/Inter (UI), Fira Code or JetBrains Mono (CLI output).

## Conventions
- Conventional Commits (no co-author attribution), feature branches, PRs per CONTRIBUTING.md. Never push to origin without explicit ask.
- Planned layout (mirrors yas-brew, the reference scaffold): `src/core/` (vendored), `src/aptadapter.*`, `src/main.cpp`, `qml/core/` (vendored) + `qml/Main.qml`, `tests/`, `assets/fonts/`, `icons/` (exists), CMakeLists.txt + CMakePresets.json.
- Packaging: `.deb` (dogfooding) + AppImage.

## Key files
README.md · DESIGN.md · CONTRIBUTING.md · EULA.md · SECURITY.md · icons/
