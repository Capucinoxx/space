class HSL {
  private hue: number;
  private saturation: number;
  private lightness: number;

  public static from_name(name: string): HSL {
    const hash = name.split('').reduce((hash, char) => char.charCodeAt(0) + ((hash << 5) - hash), 0);
    const h = Math.abs(hash % 360);
    const s = Math.abs(hash % 100) / 100;
    const l = Math.abs(hash % 50) / 100 + 0.25;

    return new HSL(h, s, l);
  }

  public static from_hex(hex: string): HSL {
    const rgb = hex.match(/[a-f0-9]{2}/gi)?.map((x) => parseInt(x, 16));
    if (rgb == undefined || rgb.length != 3)
      throw new Error(`Invalid hex color: ${hex}`);

    const r = rgb[0] / 255;
    const g = rgb[1] / 255;
    const b = rgb[2] / 255;

    const max = Math.max(r, g, b);
    const min = Math.min(r, g, b);
    const delta = max - min;

    let hue = 0;
    if (delta == 0) {
      hue = 0;
    } else if (max == r) {
      hue = 60 * (((g - b) / delta) % 6);
    } else if (max == g) {
      hue = 60 * (((b - r) / delta) + 2);
    } else if (max == b) {
      hue = 60 * (((r - g) / delta) + 4);
    }

    let saturation = 0;
    if (max == 0) {
      saturation = 0;
    } else {
      saturation = delta / max;
    }

    const lightness = (max + min) / 2;

    return new HSL(hue, saturation, lightness);
  }

  constructor(hue: number, saturation: number, lightness: number) {
    this.hue = hue;
    this.saturation = saturation;
    this.lightness = lightness;
  }

  public copy(): HSL {
    return new HSL(this.hue, this.saturation, this.lightness);
  }

  public adjust_luminosity(delta: number): HSL {
    let hsl = this.copy();
    hsl.lightness = Math.max(0, Math.min(1, delta));
    return hsl;
  }

  public to_rgba(alpha: number): string {
    const chroma = (1 - Math.abs(2 * this.lightness - 1)) * this.saturation;
    const hue_segment = this.hue / 60;
    const x = chroma * (1 - Math.abs(hue_segment % 2 - 1));

    let r = 0, g = 0, b = 0;

    switch(Math.floor(hue_segment)) {
      case 0:  r = chroma; g = x; break;
      case 1:  g = chroma; r = x; break;
      case 2:  g = chroma; b = x; break;
      case 3:  b = chroma; g = x; break;
      case 4:  b = chroma; r = x; break;
      default: r = chroma; b = x; break;
    }

    const adjustment = this.lightness - chroma / 2;
    r = Math.round((r + adjustment) * 255);
    g = Math.round((g + adjustment) * 255);
    b = Math.round((b + adjustment) * 255);

    return `rgba(${r}, ${g}, ${b}, ${alpha})`
  }

  public to_array(): [number, number, number] {
    return [this.hue, this.saturation, this.lightness];
  }
}

export { HSL }
