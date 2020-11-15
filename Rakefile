# frozen_string_literal: true

task :build do
  sh 'cmake', '-G', 'Ninja', '-B', 'build'
  sh 'cmake', '--build', 'build', '-v'
end

task :clean do
  FileUtils.rm_rf 'build'
end

task :format do
  sh 'clang-format', '-i', *Dir.glob('./**/*.*pp')
end