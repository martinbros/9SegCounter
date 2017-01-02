# A 24-bit color.
class Color
  def initialize(r, g, b)
    @r = r & 0xff
    @g = g & 0xff
    @b = b & 0xff
  end

  # Public: the hex representation of this color
  def to_hex
    "%02x%02x%02x" % [@r, @g, @b]
  end
end

# Public: create a color given r, g, v Integer values
def Color(r,g,b)
  Color.new(r,g,b)
end

