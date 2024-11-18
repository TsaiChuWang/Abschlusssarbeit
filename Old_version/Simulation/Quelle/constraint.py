import math
import numpy


# print((math.sqrt(2*numpy.log(1/0.01))*math.sqrt((covariance/mean)*(covariance/mean)*3)+(mean*3)))
# print((math.sqrt(2*numpy.log(1/0.1))*math.sqrt((covariance/mean)*(covariance/mean)*3)+(mean*3))/(mean*3))
# print((math.sqrt(2*numpy.log(1/0.01))*math.sqrt((covariance/mean)*(covariance/mean)*3)+(mean*3))/(mean*3))
# print((math.sqrt(2*numpy.log(1/0.001))*math.sqrt((covariance/mean)*(covariance/mean)*3)+(mean*3))/(mean*3))

# a = math.sqrt(2*numpy.log(1/0.1))
# b = (covariance/mean)
# a = a*math.sqrt(b*mean)
# a = a + mean
# # a = a/mean
# print(a)

capacity = 360
t = 20

# covariance = capacity/(t*t)
covariance = 40
# covariance = math.sqrt(40)
# covariance = 80
# mean = capacity/t
mean = 120

def cal(t):
    a = math.sqrt(2*numpy.log(1/0.01))
    print(a)
    b = (covariance/mean)*mean
    a = a*math.sqrt((b*b)*t)
    a = a + mean*t
    # a = a/mean
    print(a)
    print(a/360)
    print(a/t)
cal(1)
# print()
# cal(20)
# print()
# cal(30)
# print()
# cal(100)
# print()
# cal(1000)
# print()
# cal(10000)
# a = math.sqrt(2*numpy.log(1/0.001))
# b = (covariance/mean)
# a = a*math.sqrt(b*mean)
# a = a + mean

# print(a)
