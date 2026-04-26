#!/usr/bin/env python3
"""Force la souris au centre de l'ecran avec pyautogui.

Ce script est un fallback pour les environnements ou le lock SDL ne tient pas.
En mode fullscreen, le centre de l'ecran correspond au centre de la fenetre.

Usage:
  python3 tools/cowdoom_mouse_lock.py -- ./cowdoom
"""

from __future__ import annotations

import argparse
import subprocess
import sys
import time
from typing import Optional

try:
    import pyautogui
except ImportError:
    print("Erreur: pyautogui est requis. Active ton venv puis installe: python3 -m pip install pyautogui", file=sys.stderr)
    raise SystemExit(1)


def lock_loop(hz: int, tolerance_px: int, process: Optional[subprocess.Popen[bytes]]) -> None:
    delay = 1.0 / max(1, hz)
    sw, sh = pyautogui.size()
    cx = sw // 2
    cy = sh // 2

    pyautogui.FAILSAFE = False
    pyautogui.PAUSE = 0

    while True:
        if process is not None and process.poll() is not None:
            break

        mx, my = pyautogui.position()
        if abs(mx - cx) > tolerance_px or abs(my - cy) > tolerance_px:
            pyautogui.moveTo(cx, cy, duration=0)

        time.sleep(delay)


def main() -> int:
    parser = argparse.ArgumentParser(description="Lock souris au centre de l'ecran via pyautogui.")
    parser.add_argument("--hz", type=int, default=240, help="Frequence de verrouillage")
    parser.add_argument("--tolerance", type=int, default=1, help="Derive acceptee en pixels")
    parser.add_argument("command", nargs=argparse.REMAINDER, help="Commande a lancer apres '--'")
    args = parser.parse_args()

    cmd = args.command
    if cmd and cmd[0] == "--":
        cmd = cmd[1:]
    if not cmd:
        cmd = ["./cowdoom"]

    try:
        game_process: Optional[subprocess.Popen[bytes]] = subprocess.Popen(cmd)
    except OSError as exc:
        print(f"Erreur lancement jeu: {exc}", file=sys.stderr)
        return 1

    print("Mouse lock pyautogui actif (centre ecran). Ctrl+C pour arreter.")
    try:
        lock_loop(args.hz, args.tolerance, game_process)
    except KeyboardInterrupt:
        pass

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
