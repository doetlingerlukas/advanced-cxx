# frozen_string_literal: true

task :build do
  sh 'cmake', '-G', 'Ninja', '-B', 'build'
  sh 'cmake', '--build', 'build'
end

task :clean do
  FileUtils.rm_rf 'build'
end