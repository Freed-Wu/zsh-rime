{ pkgs ? import <nixpkgs> { } }:

with pkgs;
mkShell {
  name = "zsh";
  buildInputs = [
    autoconf
    gnumake
    pkg-config
    ncurses.dev
    librime
    stdenv.cc
  ];
}
