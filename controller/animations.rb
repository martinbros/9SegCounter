require_relative "color"
require_relative "serial_controller"

module Animations
  # Animation: wipe color across display a pixel at a time.
  #
  # color - the Color to wipe
  #
  def color_wipe(color)
    length.times do |num|
      self[num] = color
      show
    end
  end

  # Animation: movie theater light chaser.
  #
  #  color       - The Color value to use
  #  iterations: - number of iterations (defaults to 10)
  #  spacing:    - spacing between lights (defaults to 3)
  #
  def theater_chase(color, iterations: 10, spacing: 3)
    iterations.times do
      spacing.times do |sp|
        self.clear
        (sp...length).step(spacing) { |ii| self[ii] = color }
        show
      end
    end
  end

  # Animation: draw a rainbow that fades across all pixels at once.
  def rainbow
    0.step(255, 2) do |rainbow_pos|
      fill wheel(rainbow_pos)
      show
    end
  end

  # Animation: draw a rainbow that uniformly cycles across all pixels.
  def rainbow_cycle
    0.step(255, 2) do |pos|
      fill_each { |ii| wheel(pos - (ii * 256 / length)) }
      show
    end
  end

  # Animation: movie theater light chaser, with rainbow colors
  #
  #  spacing: - spacing between lights (defaults to 3)
  #
  def theater_chase_rainbow(spacing: 3)
    0.step(255, 8) do |pos|
      spacing.times do |space|
        self.clear
        (space...length).step(spacing) { |i| self[i] = wheel(pos) }
        show
      end
    end
  end

  # Animation: wipes a rainbow across the pixels
  def wipe_rainbow
    length.times do |num|
      0.upto(num) do |i|
        self[i] = wheel(- (i * 256 / length))
      end
      show
    end
  end

  # Internal: generate rainbow colors across 0-255 positions.
  #
  # pos - Integer position, usually between 0 and 255
  #
  # Returns a Color.
  def wheel( pos )
    pos = pos & 0xff
    if pos < 85
      # red to green
      Color(255 - pos * 3, pos * 3, 0)
    elsif pos < 170
      # green to blue
      pos -= 85
      Color(0, 255 - pos * 3, pos * 3)
    else
      # blue to red
      pos -= 170
      Color(pos * 3, 0, 255 - pos * 3)
    end
  end

end

def device
  @device ||= Dir.glob("/dev/cu.usbserial*").first
end

def strip
  return @strip if defined?(@strip)
  @strip = SerialController.new device_path: device, pixel_count: 16
  puts "starting with #{device}"
  strip.extend Animations
  @strip
end

def wipe(hex)
  if hex.length == 3
    color = Color *hex.split("").map { |c| c.ljust(2,c).hex }
  elsif hex.length == 6
    color = Color *hex.scan(/../).map(&:hex)
  else
    puts "??? #{hex}"
    return
  end
  strip.color_wipe color
end

# if animation.rb is run directly, do a long-running set of animations
if $0 == __FILE__

  stopped = false
  trap("INT") do
    if stopped
      exit!
    else
      puts "stopping..."
      stopped = true
    end
  end

  strip.color_wipe(Color(0, 0, 0))
  strip.color_wipe(Color(255, 0, 0))
  strip.color_wipe(Color(0, 255, 0))
  strip.color_wipe(Color(0, 0, 255))
  strip.wipe_rainbow
  strip.color_wipe(Color(0, 0, 0))

  exit if stopped

  strip.theater_chase_rainbow
  strip.fill Color(0, 0, 0)

  exit if stopped

  loop do
    strip.color_wipe(Color(255, 0, 0))
    3.times do
      break if stopped
      strip.rainbow
    end
    break if stopped
    strip.wipe_rainbow
    3.times do
      break if stopped
      strip.rainbow_cycle
    end
    break if stopped
  end

  strip.color_wipe(Color(0,0,0))
  exit

end
