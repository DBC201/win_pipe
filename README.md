# win_pipe

Piping utilities in c++ for windows.

Try restarting the parent pipe server when the child pipe server is throwing errors.
This happens when the child pipe server is closed without calling close().
