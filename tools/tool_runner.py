"""Bounded tool execution with local logs and reproducible input hashes."""
import hashlib
import os
import signal
import subprocess


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def run(args, directory, log, env=None):
    """Keep diagnostics and kill the driver and children if a tool stalls."""
    with log.open("wb") as output:
        process = subprocess.Popen([str(a) for a in args], cwd=directory, env=env,
                                   stdout=output, stderr=subprocess.STDOUT,
                                   start_new_session=True)
        try:
            result = process.wait(timeout=45)
        except subprocess.TimeoutExpired:
            os.killpg(process.pid, signal.SIGKILL)
            process.wait()
            raise RuntimeError(f"Tool timed out; see {log}")
    if result:
        raise RuntimeError(f"Tool exited {result}; see {log}")


