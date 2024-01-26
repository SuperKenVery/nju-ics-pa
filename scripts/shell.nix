let
    pkgs = import (builtins.fetchTarball {
        url = "https://github.com/NixOS/nixpkgs/archive/a9eb3eed170fa916e0a8364e5227ee661af76fde.tar.gz";
    }) {
      # crossSystem="i686-linux";
      system="i686-linux";
    };
    newpkgs = import <nixpkgs> {
    };
    new32pkgs = import <nixpkgs> {
      system="i686-linux";
    };
    glibc_230 = import (builtins.fetchTarball {
        url = "https://github.com/NixOS/nixpkgs/archive/136a26be29a9daa04e5f15ee7694e9e92e5a028c.tar.gz";
    }) {
      system="i686-linux";
    };

    my-py-pkgs = ps: with ps; [
      pwntools
    ];

in
(new32pkgs.buildFHSUserEnv {
  name = "fhs env";
  targetPkgs = pkgs: (with pkgs; [
    libz
    # libxcrypt
    openssl
    patchelf
    newpkgs.nix
  ]);
  multiPkgs = pkgs: (with pkgs; [
    libz
    libxcrypt
    openssl
    newpkgs.nix
  ]);
  # runScript = "bash";
}).env
