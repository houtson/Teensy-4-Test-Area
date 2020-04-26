// From: https://www.musicdsp.org/en/latest/Filters/24-moog-vcf.html

#pragma once

//namespace DistoCore
//{
  template<class T>
  class MoogFilter
  {
  public:
    MoogFilter();
    ~MoogFilter() {};

    T GetSampleRate() const { return sampleRate; }
    void SetSampleRate(T fs) { sampleRate = fs; calc(); }
    T GetResonance() const { return resonance; }
    void SetResonance(T filterRezo) { resonance = filterRezo; calc(); }
    T GetCutoff() const { return cutoff; }
    T GetCutoffHz() const { return cutoff * sampleRate * 0.5; }
    void SetCutoff(T filterCutoff) { cutoff = filterCutoff; calc(); }

    void init();
    void calc();
    T process(T input);
    // filter an input sample using normalized params
    T filter(T input, T cutoff, T resonance);
    void Process(T *sp,int n);
    
  protected:
    T sampleRate;
    // cutoff and resonance [0 - 1]
    T cutoff;
    T resonance;

    T fs;
    T y1,y2,y3,y4;
    T oldx;
    T oldy1,oldy2,oldy3;
    T x;
    T r;
    T p;
    T k;
  };

    /**
     * Construct Moog-filter.
     */
  template<class T>
  MoogFilter<T>::MoogFilter()
  : sampleRate(T(44100.0))
  , cutoff(T(1.0))
  , resonance(T(0.0))
  {
    init();
  }

    /**
     * Initialize filter buffers.
     */
  template<class T>
  void MoogFilter<T>::init()
  {
    // initialize values
    y1=y2=y3=y4=oldx=oldy1=oldy2=oldy3=T(0.0);
    calc();
  }

    /**
     * Calculate coefficients.
     */
  template<class T>
  void MoogFilter<T>::calc()
  {
    // TODO: replace with your constant
    const double kPi = 3.1415926535897931;

    // empirical tuning
    p = cutoff * (T(1.8) - T(0.8) * cutoff);
    // k = p + p - T(1.0);
    // A much better tuning seems to be:
    k = T(2.0) * sin(cutoff * kPi * T(0.5)) - T(1.0);

    T t1 = (T(1.0) - p) * T(1.386249);
    T t2 = T(12.0) + t1 * t1;
    r = resonance * (t2 + T(6.0) * t1) / (t2 - T(6.0) * t1);
  };

    /**
     * Process single sample.
     */
  template<class T>
  T MoogFilter<T>::process(T input)
  {
    // process input
    x = input - r * y4;

    // four cascaded one-pole filters (bilinear transform)
    y1 =  x * p + oldx  * p - k * y1;
    y2 = y1 * p + oldy1 * p - k * y2;
    y3 = y2 * p + oldy2 * p - k * y3;
    y4 = y3 * p + oldy3 * p - k * y4;

    // clipper band limited sigmoid
    y4 -= (y4 * y4 * y4) / T(6.0);

    oldx = x; oldy1 = y1; oldy2 = y2; oldy3 = y3;

    return y4;
  }

  template<class T>
  void MoogFilter<T>::Process(T *sp,int n)
  {
    for(int i = 0;i < n;i++) {
      *sp = process(*sp);
      sp++;
    }
  }
  
    /**
     * Filter single sample using specified params.
     */
  template<class T>
  T MoogFilter<T>::filter(T input, T filterCutoff, T filterRezo)
  {
    // set params first
    cutoff = filterCutoff;
    resonance = filterRezo;
    calc();

    return process(input);
  }
//}
