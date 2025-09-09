## Mailing List Guidelines

Your patches should be sent to the address for the specific assignment.
Each assignment will list the appropriate email and the correct command will look something like:

```
$ git send-email --to=whatever_assignment@fall2026-uml.kdlp.underground.software v1*.patch
```

This command attempts to send any file in the current directory starting with `v1`
and ending with `.patch` to the mailing list. `v1*.patch` is an example of `glob`
expansion in the shell. See `man 7 glob` for more information.
