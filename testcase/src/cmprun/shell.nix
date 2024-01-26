with import <nixpkgs> {};
let 
  my-py-packages = ps: with ps; [
    # (
    #   buildPythonPackage rec {
    #     pname="pyrsp";
    #     version="e21c1584e9c00b12f70f31ee5f4b545b1c5e0419";
    #     src=fetchFromGitHub {
    #       owner="SuperKenVery";
    #       repo="${pname}";
    #       rev="e21c1584e9c00b12f70f31ee5f4b545b1c5e0419";
          
    #       sha256="sha256-LeCEPFsn+mLoRxe2xVZwY8JF+fXu1oEOf8DwGLDDM/M=";
    #     };
    #     doCheck=false;
    #     propagetedBuildInputs= with python311Packages; [
    #       pyelftools
    #       pyserial
    #       construct
    #       six
    #       psutil
    #     ];
    #   }
    # )
    # pyelftools
    # pyserial
    # construct
    # six
    # psutil

    pygdbmi
    pwntools
  ];
in
mkShell {
  name="qemu runner python env";
  buildInputs = with pkgs; [
    (python311.withPackages my-py-packages)
    qemu
  ];
}
