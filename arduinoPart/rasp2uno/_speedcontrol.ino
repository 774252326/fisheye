
float SpeedOutput(float input, float minInput, float minSpeed, float maxInput, float maxSpeed)
{
    if(input<=minInput)
        return minSpeed;
    if(input>=maxInput)
        return maxSpeed;
    return minSpeed+(input-minInput)*(maxSpeed-minSpeed)/(maxInput-minInput);
}


void xyz2speed(int &ySpeed, int &spinSpeed, float x, float y, float z, float yUpRatio, float spinUpRatio)
{
    ySpeed=ySpeed*yUpRatio+(1-yUpRatio)*SpeedOutput(z, 600, 0, 1000, 90);
    spinSpeed=spinSpeed*spinUpRatio+(1-spinUpRatio)*SpeedOutput(atan(x/z), -0.8, -40, 0.8, 40);
}
