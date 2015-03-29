func SomeIOFunction(path string) {
    file, err := os.Open(path)
    if err != nil {
        log.Fatal(err)  // os.Open returned an error
    }
    err = pkg.SomeIOFunc(file)
    if err != nil {
        log.Fatal(err) // rinse and repeat
    }
}
