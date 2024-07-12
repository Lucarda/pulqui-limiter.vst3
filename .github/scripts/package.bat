@echo off
del Pulquilimiter-%1.zip /Q
7z a Pulquilimiter-%1.zip PulquiLimiter.vst3
@echo on
