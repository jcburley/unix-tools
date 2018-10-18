# fupos

File-unit Positioning Tools

## Example

In Bash (or perhaps zsh):

```
$ { read FOO; ./seek 0 0; read BAR; } <<< $'test line 1\ntest line 2'
$ echo $FOO
test line 1
$ echo $BAR
test line 1
$
```
