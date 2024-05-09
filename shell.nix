{ pkgs ? import <nixpkgs> { } }:

with pkgs;
mkShell {
  name = "zsh";
  buildInputs = [
    ncurses.dev
  ];
}
