#!/bin/bash

rm -f Pulquilimiter-$1.zip
zip -r -q Pulquilimiter-$1.zip PulquiLimiter.vst3
rm -r PulquiLimiter.vst3
