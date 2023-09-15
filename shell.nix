with import <nixpkgs> {};

pkgsi686Linux.stdenv.mkDerivation {
  name="i686 cross compile environment";

  buildInputs = [
    gcc
    clang-tools_16
  ];
}
