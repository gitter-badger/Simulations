# SConscript for my simulations.

Import('env')

# Build libvb and the associated environment.

fltk = env.Clone()

if fltk['GUI'] == 'fltk':
    try:
        fltk.ParseConfig('fltk2-config --cxxflags --ldflags')
        fltk.Append (CXXFLAGS = ["-DLIBVB_FLTK"], LIBS = ["png"])
    except OSError:
        print "FLTK2 not found, building without display support."
        fltk['GUI'] = None

fltk.Append ( CPPPATH = ["#libvb"] )

libvb = SConscript ("libvb/SConscript", exports="fltk")

vb = fltk.Clone()
vb.Append ( LIBPATH = [ libvb[0].dir ], LIBS = ["vb"] )

# Now build the programs.

for i in ['tests', '1D', '2D', 'trucs']:
  SConscript ("%s/SConscript" % i, exports="vb")

SConscript ("xtoys/SConscript", exports="env")

# Install the headers

env.Install ("$prefix/include", "libvb/vb.h")
env.Install ("$prefix/include/vb", Glob("libvb/vb/*.h"))
