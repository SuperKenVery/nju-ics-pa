let
    pkgs = import (builtins.fetchTarball {
        url = "https://github.com/NixOS/nixpkgs/archive/a9eb3eed170fa916e0a8364e5227ee661af76fde.tar.gz";
    }) {
      # crossSystem="i686-linux";
      system="i686-linux";
    };

    myPkg = pkgs.gcc-unwrapped;
in
  pkgs.stdenv.mkDerivation {
    name="gcc8";
    buildInputs=[
      pkgs.gcc
    ];
  }
