with import <nixpkgs> {};

pkgsi686Linux.stdenvNoCC.mkDerivation {
  name="i686 cross compile environment";

  buildInputs = [
    gcc8
    clang-tools_16
    readline70
  ];
}
