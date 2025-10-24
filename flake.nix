{
  description = "Rofi - Optimized build";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
        in
        {
          default = pkgs.rofi-wayland.overrideAttrs (old: {
            pname = "rofi-optimized";
            version = "2.0.0-next";

            src = ./.;

            mesonFlags = (old.mesonFlags or []) ++ [
              "-Dbuildtype=release"
            ];

            # Our custom optimization and hardening flags are in meson.build
          });
        });

      nixosModules.default = { config, lib, pkgs, ... }:
        with lib;
        let
          cfg = config.programs.rofi-optimized;
        in
        {
          options.programs.rofi-optimized = {
            enable = mkEnableOption "optimized rofi";
          };

          config = mkIf cfg.enable {
            environment.systemPackages = [ self.packages.${pkgs.system}.default ];
          };
        };
    };
}
