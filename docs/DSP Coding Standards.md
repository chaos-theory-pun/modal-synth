# DSP Coding Standards

## DSP Classes

- All DSP classes meet certain requirements
- CamelCase names for classes, snake_case for methods & members
- Informal interface, could be formal but no need for now
- All DSP classes should keep their high-level parameters, not just coefficients
- Implements following methods:
- - `num tick([num in])`, process and return a single sample, argument is optional if is e.g. oscillator that creates samples
- - `void set_sample_rate(num sr)`, set sample rate to new, propagate to member objects, and update coefficients
- - `void set_params([...])`, or alternatively several different `set_param()` methods if there are too many for one method call or if some coefficients don't need all params to calc 

## Instrument Classes

- For instruments (currently only `ModalSynth`) so they can work with `PolyController`
- Should do all the DSP class things
- And implement `void on(num freq, num vel)` and `void off()`