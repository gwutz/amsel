tests = [
  'engine',
  'utils',
  'stresstest',
]

test_deps = [
  dependency('glib-2.0'),
  dependency('libxml-2.0'),
  dependency('libsoup-2.4'),
]

foreach t : tests
  exe = executable(t, ['@0@.c'.format(t)], dependencies: test_deps,
    link_with: alb_lib,
    include_directories: inc,
  )

  test(t, exe, is_parallel: false)
endforeach

# Cache test
exe = executable('cache', ['cache.c', 'amsel-memory-database.c'], dependencies: test_deps,
  link_with: alb_lib,
  include_directories: inc,
)

test('cache', exe, is_parallel: false)

# Database test
exe = executable('database', ['database.c', 'amsel-memory-database.c'], dependencies: test_deps,
  link_with: alb_lib,
  include_directories: inc,
)

test('database', exe, is_parallel: false)

# exe = executable('store', ['store.c', '../src/amsel-store.c'],
#   dependencies: [
#     libamsel_dep,
#     dependency('gtk+-3.0', version: '>= 3.22'),
#   ])

# test('store', exe)