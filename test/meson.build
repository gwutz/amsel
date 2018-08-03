tests = [
  'engine',
  'utils',
]

test_deps = [
  dependency('glib-2.0'),
]

foreach t : tests
  exe = executable(t, ['@0@.c'.format(t)], dependencies: test_deps,
    link_with: amsel_lib,
    include_directories: inc,
  )

  test(t, exe)
endforeach

# exe = executable('store', ['store.c', '../src/amsel-store.c'],
#   dependencies: [
#     libamsel_dep,
#     dependency('gtk+-3.0', version: '>= 3.22'),
#   ])

# test('store', exe)