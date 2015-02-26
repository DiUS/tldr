module TLDR

  class Command

    COMMANDS = ["fd", "bk", "rt", "lt"]

    def initialize(device)
      @device = device
      @device.sync = true
      @device.read_timeout = 50
      @device.read(100)
    end

    def run(input)
      command = input.read
      if command =~ /repeat/
        command = command.gsub(/repeat (\d+) \[(.*)?\]/) do
          times = Regexp.last_match[1]
          inner_commands = split_commands Regexp.last_match[2]
          "repeat(#{times}) {#{inner_commands}}"
        end
      end
      instance_eval(command)
    end

    def repeat(times, &block)
      times.times { block.call }
    end

    def split_commands command
      command.split.each_slice(2).collect{ |slice| slice.join(" ") }.join("; ")
    end

    def method_missing(method, *args, &block)
      if COMMANDS.include?(method.to_s.downcase)
        command = method.to_s + " " + args[0].to_s + "\n"
        while !send_command(command) do end
        # send_command(command)
      end
    end

    def send_command(command)
      command.chars.each do |c|
        @device.putc c
        sleep 0.2
      end
      output = @device.read(10)
      result = "0"
      if output
        result = output.chomp
      end
      return result.to_i == 0
    end

  end

end
