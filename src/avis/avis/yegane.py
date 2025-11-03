import numpy as np
import cv2

class LaneDetector:
    def __init__(self, width_of_line=250, degree=0, distance=0, curve=0):
        self.width_of_line = width_of_line +5
        self.degree = degree
        self.distance = distance
        self.curve = curve
        self.any_road = False

    def line_detection(self, warped):
        gray_frame = cv2.cvtColor(warped, cv2.COLOR_BGR2GRAY)

        blurred = cv2.GaussianBlur(gray_frame, (5, 5), 0)

        _, thresh = cv2.threshold(blurred, thresh=200, maxval=255, type=cv2.THRESH_TOZERO)
        #adaptive = cv2.adaptiveThreshold(
            #blurred, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 15, -10)

        #edge = cv2.Canny(adaptive, 75, 200)
        
        cv2.imshow("thresh", thresh)
        return thresh


    def single_line(self,img, histogram, check):

        midpoint = int(histogram.shape[0] // 2)
        leftx_base = np.argmax(histogram[:midpoint])

        nwindows = 10
        minpix = 50
        window_width = 80
        window_height = int(img.shape[0] // nwindows)

        nonzero = img.nonzero()
        nonzeroy = np.array(nonzero[0])
        nonzerox = np.array(nonzero[1])

        onex_current = leftx_base
        one_lane_inds = []

        for window in range(nwindows):
            win_y_low = img.shape[0] - (window + 1) * window_height
            win_y_high = img.shape[0] - window * window_height
            win_xone_l = onex_current - window_width
            win_xone_r = onex_current + window_width

            good_line_inds = ((nonzeroy >= win_y_low) & (nonzeroy < win_y_high) &
                              (nonzerox >= win_xone_l) & (nonzerox < win_xone_r)).nonzero()[0]

            one_lane_inds.append(good_line_inds)
    
            if len(good_line_inds) > minpix:
                onex_current = int(np.mean(nonzerox[good_line_inds]))
  
        try:
            one_lane_inds = np.concatenate(one_lane_inds)
        except ValueError:
            pass

        linex = nonzerox[one_lane_inds]
        liney = nonzeroy[one_lane_inds]
        
        line_fit = np.polyfit(liney, linex, 2)

        width = img.shape[1]
        height = img.shape[0]
        ploty = np.linspace(0, height - 1, height)

        try:
            line_fitx = line_fit[0] * ploty ** 2 + line_fit[1] * ploty + line_fit[2]
        except TypeError:
            print('The function failed to fit a line!')
            line_fitx = 1 * ploty ** 2 + 1 * ploty

        if self.width_of_line < 250 or self.width_of_line > 350:
            self.width_of_line = 300

        if check == "right":
            center_fitx = (line_fitx - (self.width_of_line/2))
        elif check == "left":
            center_fitx = (line_fitx + (self.width_of_line/2))

        center_fit = [line_fit[0], line_fit[1], line_fit[2]]

        road = np.zeros((height, width, 3), dtype=np.uint8)
        cv2.polylines(road, [np.int32(np.column_stack((line_fitx, ploty)))], False, (255, 0, 0), 5)
        cv2.polylines(road, [np.int32(np.column_stack((center_fitx, ploty)))], False, (255, 0, 255), 5)
        cv2.line(road, (250,0), (250,199), (0,200,200), 2)

        return center_fit, center_fitx, ploty, road

    def set_line (self, img, histogram, check, method):

        midpoint = int(histogram.shape[0] // 2)

        if check == "left":
            linex_base = np.argmax(histogram[:midpoint])
        elif check == "right":
            linex_base = np.argmax(histogram[midpoint:]) + midpoint

        nwindows = 10
        minpix = 50
        window_width = 80
        window_height = int(img.shape[0] // nwindows)

        nonzero = img.nonzero()
        nonzeroy = np.array(nonzero[0])
        nonzerox = np.array(nonzero[1])

        linex_current = linex_base

        one_lane_inds = []

        for window in range(nwindows):
            win_y_low = img.shape[0] - (window + 1) * window_height
            win_y_high = img.shape[0] - window * window_height
            win_xline_l = linex_current - window_width
            win_xline_r = linex_current + window_width
        
            good_line_inds = ((nonzeroy >= win_y_low) & (nonzeroy < win_y_high) &
                              (nonzerox >= win_xline_l) & (nonzerox < win_xline_r)).nonzero()[0]

            one_lane_inds.append(good_line_inds)

            if len(good_line_inds) > minpix:
                linex_current = int(np.mean(nonzerox[good_line_inds]))

        try:
            one_lane_inds = np.concatenate(one_lane_inds)
        except ValueError:
            pass

        linex = nonzerox[one_lane_inds]
        liney = nonzeroy[one_lane_inds]

        height = img.shape[0]
        ploty = np.linspace(0, height - 1, height)

        if method == 1:
            line_fit_linear = np.polyfit(liney, linex, 1)

            line_fitx = line_fit_linear[0] * ploty + line_fit_linear[1]
            line_fit = np.array([0, line_fit_linear[0], line_fit_linear[1]])
       
        elif method == 2:
            line_fit = np.polyfit(liney, linex, 2)
            line_fitx = line_fit[0] * ploty ** 2 + line_fit[1] * ploty + line_fit[2]
        
        return line_fit, line_fitx, ploty
    
    def set_center_line(self, left_fit, left_fitx, right_fit, right_fitx, ploty):

        width = 500
        height = 200

        center_fitx = (left_fitx + right_fitx) // 2

        center_fit = [
            (left_fit[0] + right_fit[0]) / 2,
            (left_fit[1] + right_fit[1]) / 2,
            (left_fit[2] + right_fit[2]) / 2
        ]

        self.width_of_line = right_fitx[-1] - left_fitx[-1]

        road = np.zeros((height, width, 3), dtype=np.uint8)
        cv2.polylines(road, [np.int32(np.column_stack((left_fitx, ploty)))], False, (255, 0, 0), 5)
        cv2.polylines(road, [np.int32(np.column_stack((right_fitx, ploty)))], False, (0, 0, 255), 5)
        cv2.polylines(road, [np.int32(np.column_stack((center_fitx, ploty)))], False, (0, 255, 0), 5)
        cv2.line(road, (250,0), (250,199), (0,200,200), 2)

        return center_fit, center_fitx, ploty, road
    

    def find_lane(self, img):

        self.any_road = False
        histogram = np.sum(img[100:, :], axis=0)
        midpoint = int(histogram.shape[0] // 2)
        midpoint += int(self.distance//10)

        left_is_center = False
        if np.sum(img[170:, :midpoint]) > np.sum(img[170:, midpoint:]):
            left_is_center = True

        Left_is_ok = True
        Right_is_ok = True

        if(max(histogram[:midpoint])>7000):
            left_fit, left_fitx, ploty = self.set_line(img, histogram, "left", 2)
        elif(max(histogram[:midpoint])>5000):
            left_fit, left_fitx, ploty = self.set_line(img, histogram, "left", 1)
        else:
            Left_is_ok = False
        if(max(histogram[midpoint:])>3000):
            right_fit, right_fitx, ploty = self.set_line(img, histogram, "right", 2)
        elif(max(histogram[midpoint:])>2000):
            right_fit, right_fitx, ploty = self.set_line(img, histogram, "right", 1)
        else:
            Right_is_ok = False
        if Right_is_ok and Left_is_ok:
            center_fit, center_fitx, ploty, road = self.set_center_line(left_fit, left_fitx, right_fit, right_fitx, ploty)
        elif not Right_is_ok and Left_is_ok:
            center_fit, center_fitx, ploty, road = self.single_line(img, histogram, "right")
        elif not Left_is_ok and Right_is_ok:
            center_fit, center_fitx, ploty, road = self.single_line(img, histogram, "left")
        else:
            self.any_road = True
            road = np.zeros((200, 500, 3), dtype=np.uint8)
            center_fit, center_fitx, ploty = 0, 0, 0
        
        return center_fit, center_fitx, ploty, road, self.any_road, left_is_center

    def fit_polynomial(self, img):
        center_fit, center_fitx, ploty, road, self.any_road, left_is_center = self.find_lane(img)

        if not self.any_road:
 
            d_center = np.polyder(center_fit)
            center_y = ploty[-20:]

            distance, w1, w2, degree = 0, 20, 20, 0
            last_20 = center_fitx[-20:]

            for i in range(20):
                d = last_20[i]
                distance += (w1 * (d - (500 // 2)))/210
                w1 -= 1

                y = center_y[i]
                m_center = np.polyval(d_center, y)
                theta = (np.degrees(np.arctan(m_center))*w2)/210
                degree += theta
                w2 -= 1

            self.degree = degree
            self.distance = distance
            self.curve = center_fit[0]

        return road, self.distance, -self.degree, abs(self.curve)*100000, left_is_center

    def process_frame(self, frame):
        frame = frame[300:500, :]
        frame = cv2.resize(frame, (500, 200))

        width, height = frame.shape[1], frame.shape[0]

        src_points = np.array([[100, 50], [400, 50], [500, 190], [0, 190]], dtype="float32")
        dst_points = np.array([[0, 50], [500, 50], [500, 190], [0, 190]], dtype="float32")

        matrix = cv2.getPerspectiveTransform(src_points, dst_points)
        warped = cv2.warpPerspective(frame, matrix, (width, height))

        clean_line = self.line_detection(warped)
        road, distance, degree, curve, left_is_center = self.fit_polynomial(clean_line)

        Minv = cv2.getPerspectiveTransform(dst_points, src_points)
        road_w = cv2.warpPerspective(road, Minv, (width, height))

        result = cv2.addWeighted(frame, 1, road_w, 1, 0)
        road_on_warped = cv2.addWeighted(warped, 1, road, 1, 0)

        cv2.putText(result, f"Distance : {round(distance, 2)}", (10, 20),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        cv2.putText(result, f"Degree : {round(degree, 2)}", (10, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        cv2.putText(result, f"Curve : {round(curve, 2)}", (10, 60),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        #cv2.putText(result, f"width : {round(self.width_of_line, 2)}", (10, 80),
                    #cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        cv2.putText(result, f"left is center : {left_is_center}", (10, 100),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
        
        cv2.line(road, (width//2, 0), (width//2, height), (0,100,100), 2)
        
        for pt in src_points.astype(int):
            cv2.circle(result, tuple(pt), 3, color=(0, 0, 255), thickness=-1)

        cv2.imshow("road", road_on_warped)
        cv2.imshow("result", result)
        #cv2.imshow("out_img", road)
        return road, distance, degree, curve

