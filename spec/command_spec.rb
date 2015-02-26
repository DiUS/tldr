require 'serialport'

require 'spec_helper'

describe "command" do

  let(:data) {}

  xit "should do" do
    output = StringIO.new
    logo = TLDR::Command.new(StringIO.new("fd 200"), output)
    expect(output.string).to eq("fd200")
  end

  it "should do blue" do
    output = SerialPort.new("/dev/tty.HC-05-DevB", 9600, 8, 1, SerialPort::NONE)
    output.sync = true
    logo = TLDR::Command.new(StringIO.new("fd 200"), output)
    # expect(output.string).to eq("fd200")
  end

end
