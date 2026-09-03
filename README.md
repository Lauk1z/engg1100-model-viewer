# ENGG1100 interactive model

Static GitHub Pages package for the ENGG1100 flood-resistant housing concept.

The page loads `assets/engg1100-flood-resistant-housing.glb` with Google's
`<model-viewer>` component. Publish this directory as the root of a GitHub Pages
repository. No build step is required.

## Arduino dual-winch firmware / 双绞盘驱动

The Arduino UNO R4 Minima firmware for the XC4492/KS0063 L298N driver and
three-position I-O-II rocker switch is available in
[`firmware/ENGG1100_Arduino_Winch`](firmware/ENGG1100_Arduino_Winch/README.md).

- `I`: pay out / 放绳
- `O`: stop / 停止
- `II`: reel in / 收绳
