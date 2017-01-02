require_relative "color"

class SerialController
  attr_reader :length
  attr_reader :device

  def initialize(device_path: , pixel_count:)
    @device = File.open(device_path, 'r+')
    @length = pixel_count
    clear_read_buffer
    clear
  end

  def clear_read_buffer
    loop do
      @device.read_nonblock(1024)
    end
  rescue IO::EAGAINWaitReadable
  end

  def []=(i, color)
    @pixels[i] = color
  end

  def fill(color)
    length.times { |i| self[i] = color }
    self
  end

  def fill_each
    length.times do |i|
      if color = yield(i)
        self[i] = color
      end
    end
  end

  def clear
    @pixels = Array.new(length, Color(0, 0, 0))
    self
  end

  def show
    clear_read_buffer
    # print "--> "
    # start = Time.now.to_f
    @device.print "x" # reset the input
    data = @pixels.map(&:to_hex).join
    @device.print data # print the data
    @device.gets # ack reset
    @device.gets # ack written
    # puts "%0.3f" % ((Time.now.to_f - start) * 1000)
  ensure
    clear_read_buffer
  end

end
