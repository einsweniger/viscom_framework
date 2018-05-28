uniform sampler1D texFFTSmoothed;
uniform sampler1D texFFTIntegrated;
uniform sampler1D texFFT;
uniform bool test_fft_smooth = false;
uniform float test_fft_scale = 50;



float get_average(sampler1D soundTex, int texel, int bincount) {
    int ty = int(texCoord.y*1024.0);
    int samplestart = -(bincount%2);
    int sampleend = abs(samplestart);
    float sumfreq =0;
    for(int i = samplestart; i != sampleend; i++) {
      sumfreq += texelFetch(soundTex, texel+i,0).x/0.070;
    }
    sumfreq = sumfreq/bincount;
    return sumfreq;
}

//vec4 sound_test_color() {
//  vec4 out_color;
//
//  int tx = int(texCoord.x*750.0);
//  int ty = int(texCoord.y*1024.0);
//  //out_color = vec4(col,1);
//  float freq = texelFetch(texFFTSmoothed, tx,0).x/0.070;
//  int samplestart = -(bincount%2);
//  int sampleend = abs(samplestart);
//  float sumfreq =0;
//  for(int i = samplestart; i != sampleend; i++) {
//      sumfreq += texelFetch(texFFTSmoothed, tx+i,0).x/0.070;
//  }
//  sumfreq = sumfreq/bincount;
//
//  //freq = toLog(freq, 0.01, 1);
//  out_color = vec4(0);
//  out_color.w = 1.0;
//  if (sumfreq > texCoord.y) {
//    out_color  =vec4(1);
//  }
//
//  return out_color;
//}
