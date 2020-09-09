# ToDo
Commandline to-do list interface (unix)

## Build
```
make
```

## Usage
```
todo [action] -a [listname] -[item modify flags] -Isx
```
### Flags
- 'a' specifies which list to modify, create, delete, etc...
- 's' sorts the list by date-time
- 'i' shows only flagged "important" list items
- 'x' shows extra information about a list

List item data flags:
- 'T' the list item name
- 'm' the list item description
- 'd' the list item date
- 't' the list item time
- 'p' the list item status (0: unfinished, 1-99: WIP, 100: finished)
- 'f' the list item flag for "important"

### Notes
- If no list name is given using the 'a' flag, the first list in the directory (if any) will be targeted.

## Docs
- 'ls' lists all to-do list saved in the current directory
- 'new' [-a listname] creates a new list with name listname
- 'add' [item modify flags] adds a new item to a list