# commando

Prints a random command from a list every time you open a shell, so you actually remember the ones you keep forgetting.

## Installation

```bash
./install.sh
```

Compiles the binary and appends a call to your `~/.bashrc` so it runs on shell start. You'll be prompted before anything is written.

**Requirements:** `g++` with C++20 support.

## Commands file

Commands are defined in `commands.txt` (or any file you point at). Format:

```
- your-command
  Description of what it does. Can span multiple lines.
  Mention your-command again and it'll be highlighted in green.

- another-command
  Use <red>, <green>, or <blue> tags for colour.
  Close them with </red>, </green>, or </blue>.

// This is a comment and will be ignored
```

Lines starting with `-` are commands. Everything below until the next `-` is the description.

## Usage

```bash
commando /path/to/commands.txt
```
