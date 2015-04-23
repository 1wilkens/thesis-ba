const conversionFactor = math.Pi / 180

func Rad(deg float64) float64 {
    return deg * conversionFactor
}

func HaversineLength(n1, n2 *Node) float64 {
    lat1, lon1, lat2, lon2 :=
      Rad(n1.lat), Rad(n1.lon), Rad(n2.lat), Rad(n2.lon)
    dlat, dlon := lat2-lat1, lon2-lon1
    a := math.Pow(math.Sin(dlat/2), 2) + math.Cos(lat1) *
        math.Cos(lat2) * math.Pow(math.Sin(dlon/2), 2)
    c := 2 * math.Asin(math.Sqrt(a))
    return 6367000 * c // distance in m
}
