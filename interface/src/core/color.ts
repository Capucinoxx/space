class HSL {
  private hue: number;
  private saturation: number;
  private lightness: number;

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
}

export { HSL }
