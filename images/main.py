import cv2


def main():
    img = cv2.imread("background.png")
    print(img[0][0])


main()
