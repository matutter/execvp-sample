
env = Environment()

env.Append(
  CPPFLAGS="-Isrc"
)

cp_obj = SConscript(
  'src/SConscript',
  variant_dir='build',
  exports='env',
  duplicate=0
)

objects = cp_obj

tests = SConscript(
  'test/SConscript',
  variant_dir='build/test',
  exports='env objects',
  duplicate=0
)

#Depends(tests, sharedObject)
Alias('tests', tests)

Default(objects)
