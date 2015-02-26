
require 'spec_helper'

describe "command" do

  let(:data) {}

  it "should do" do
    output = StringIO.new
    logo = TLDR::Command.new(StringIO.new("fd 200"), output)
    expect(output.string).to eq("fd200")
  end

end
