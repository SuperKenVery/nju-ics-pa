let
    pkgs = import (builtins.fetchTarball {
        url = "https://github.com/NixOS/nixpkgs/archive/a9eb3eed170fa916e0a8364e5227ee661af76fde.tar.gz";
    }) {
      # crossSystem="i686-linux";
      system="i686-linux";
    };
    newpkgs = import <nixpkgs> {
    };
    newi686pkgs = import <nixpkgs> {
      system="i686-linux";
    };

in
  pkgs.stdenv.mkDerivation {
    name="gcc8";
    buildInputs=with pkgs; [
      gcc
      readline
      SDL
      bear
      newi686pkgs.glibc
      newpkgs.pwndbg
      newpkgs.glibc
    ];

    # Nix-ld
    NIX_LD_LIBRARY_PATH = newi686pkgs.lib.makeLibraryPath [
      newi686pkgs.stdenv.cc.cc
    ];
    NIX_LD = newi686pkgs.lib.fileContents "${newi686pkgs.stdenv.cc}/nix-support/dynamic-linker";
  }
