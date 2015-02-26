
require 'tldr'

RSpec.configure do |config|
  config.color = true
  config.tty = true
  config.formatter = :documentation # :progress, :html
  # config.fail_fast =true
end
