module TLDR

  class Command

    COMMANDS = ["fd"]

    def initialize(input, output)
      @output = output
      instance_eval(input.read)
    end

    def method_missing(method, *args, &block)
      if COMMANDS.include?(method.to_s.downcase)
        @output.print method.to_s + args[0].to_s
      end
    end

  end

end
