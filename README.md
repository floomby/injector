injector
========

does reflective dll injection on 64 bit windows.

Right now a hodge podge of dll things that
does not actualy do reflective dll injection
however they all work and if all the techniques
that are being used in the things get combined
correctly reflective dll injection can be done.

It should be able to do reflective dll
injection by 1/20 or sooner.

Plans include a creation of tool to take a
existing exploit and a dll and create a payload
from that.

I plan to write a defualt payload that fetures
encrytpted networking, process migration, uses
staging and hot-patching of code. This would come
with a handler and a an assortment of modlules
that can be hot-patched into it over the network
to provide various functionality, and the proper
documantation to develop modlules for the payload.

build notes:
The toolchain that I use is a bit wonky; you
need to have 64-bit cygwin and the 64 bit gcc
of at least version 4.8 that ships as a cygwin
package.
