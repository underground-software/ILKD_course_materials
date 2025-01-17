## Mailing List Guidelines

Your patches should be sent to the address for the specific assignment.
Each assignment will list the appropriate email to use like so:

```
$ git send-email --to=whatever_assignment@spring2025-utsa.kdlp.underground.software *.patch
```

This command attempts to send any file in the current directory ending in
`.patch` to the mailing list. `*.patch` is an example of `glob` expansion
in the shell. See `man 7 glob` for more information.

