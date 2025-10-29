import numpy as np
import cv2

class LaneDetector:
    def __init__(self):
        pass

    def line_detection(self, warped):
        gray_frame = cv2.cvtColor(warped, cv2.COLOR_BGR2GRAY)
        hsv = cv2.cvtColor(warped, cv2.COLOR_RGB2HSV)
        hls = cv2.cvtColor(warped, cv2.COLOR_RGB2HLS)

        blurred = cv2.GaussianBlur(gray_frame, (7, 7), 0)

        _, thresh = cv2.threshold(blurred, thresh=100, maxval=255, type=cv2.THRESH_TOZERO)
        adaptive = cv2.adaptiveThreshold(
            blurred, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 15, -10
        )

        edge = cv2.Canny(adaptive, 75, 200)

        return adaptive

    def find_lane(self, img):
        histogram = np.sum(img[img.shape[0] // 4:, :], axis=0)

        midpoint = int(histogram.shape[0] // 2)
        leftx_base = np.argmax(histogram[:midpoint])
        rightx_base = np.argmax(histogram[midpoint:]) + midpoint

        # Windows properties
        nwindows = 10
        minpix = 50
        window_width = 20
        window_height = int(img.shape[0] // nwindows)

        nonzero = img.nonzero()
        nonzeroy = np.array(nonzero[0])
        nonzerox = np.array(nonzero[1])

        # Current positions to be updated later
        leftx_current = leftx_base
        rightx_current = rightx_base

        left_lane_inds = []
        right_lane_inds = []

        out_img = np.dstack((img, img, img))

        for window in range(nwindows):
            win_y_low = img.shape[0] - (window + 1) * window_height
            win_y_high = img.shape[0] - window * window_height
            win_xleft_l = leftx_current - window_width
            win_xleft_r = leftx_current + window_width
            win_xright_l = rightx_current - window_width
            win_xright_r = rightx_current + window_width

            cv2.rectangle(out_img, (win_xleft_l, win_y_low),
                          (win_xleft_r, win_y_high), (0, 255, 0), 2)
            cv2.rectangle(out_img, (win_xright_l, win_y_low),
                          (win_xright_r, win_y_high), (0, 255, 0), 2)

            good_left_inds = ((nonzeroy >= win_y_low) & (nonzeroy < win_y_high) &
                              (nonzerox >= win_xleft_l) & (nonzerox < win_xleft_r)).nonzero()[0]

            good_right_inds = ((nonzeroy >= win_y_low) & (nonzeroy < win_y_high) &
                               (nonzerox >= win_xright_l) & (nonzerox < win_xright_r)).nonzero()[0]

            left_lane_inds.append(good_left_inds)
            right_lane_inds.append(good_right_inds)

            if len(good_left_inds) > minpix:
                leftx_current = int(np.mean(nonzerox[good_left_inds]))
            if len(good_right_inds) > minpix:
                rightx_current = int(np.mean(nonzerox[good_right_inds]))

        try:
            left_lane_inds = np.concatenate(left_lane_inds)
            right_lane_inds = np.concatenate(right_lane_inds)
        except ValueError:
            pass

        leftx = nonzerox[left_lane_inds]
        lefty = nonzeroy[left_lane_inds]
        rightx = nonzerox[right_lane_inds]
        righty = nonzeroy[right_lane_inds]

        return leftx, lefty, rightx, righty, out_img

    def fit_polynomial(self, img):
        leftx, lefty, rightx, righty, out_img = self.find_lane(img)

        left_fit = np.polyfit(lefty, leftx, 2)
        right_fit = np.polyfit(righty, rightx, 2)

        width = img.shape[1]
        height = img.shape[0]
        ploty = np.linspace(0, height - 1, height)

        try:
            left_fitx = left_fit[0] * ploty ** 2 + left_fit[1] * ploty + left_fit[2]
            right_fitx = right_fit[0] * ploty ** 2 + right_fit[1] * ploty + right_fit[2]
        except TypeError:
            print('The function failed to fit a line!')
            left_fitx = 1 * ploty ** 2 + 1 * ploty
            right_fitx = 1 * ploty ** 2 + 1 * ploty

        center_fitx = (left_fitx + right_fitx) // 2

        center_fit = [
            (left_fit[0] + right_fit[0]) / 2,
            (left_fit[1] + right_fit[1]) / 2,
            (left_fit[2] + right_fit[2]) / 2
        ]

        d_center = np.polyder(center_fit)
        center_y = ploty[-20:]

        distance, w1, w2, degree = 0, 20, 20, 0
        last_20 = center_fitx[-20:]

        for i in range(20):
            d = last_20[i]
            distance += (w1 * (d - (width // 2)))/210
            w1 -= 1


            y = center_y[i]
            m_center = np.polyval(d_center, y)
            theta = (np.degrees(np.arctan(m_center))*w2)/210
            degree += theta
            w2 -= 1

        out_img[lefty, leftx] = [255, 0, 0]
        out_img[righty, rightx] = [0, 0, 255]

        road = np.zeros((height, width, 3), dtype=np.uint8)
        cv2.polylines(road, [np.int32(np.column_stack((left_fitx, ploty)))], False, (255, 0, 0), 5)
        cv2.polylines(road, [np.int32(np.column_stack((right_fitx, ploty)))], False, (0, 0, 255), 5)
        cv2.polylines(road, [np.int32(np.column_stack((center_fitx, ploty)))], False, (0, 255, 0), 5)

        #cv2.imshow("windows", out_img)

        return road, distance, -degree

    def process_frame(self, frame):
        frame = cv2.resize(frame, (500, 300))
        #cv2.imshow("original", frame)

        width, height = frame.shape[1], frame.shape[0]

        src_points = np.array([[160, 170], [400, 170], [500, 230], [50, 230]], dtype="float32")
        dst_points = np.array([[50, 100], [450, 100], [450, 300], [50, 300]], dtype="float32")

        matrix = cv2.getPerspectiveTransform(src_points, dst_points)
        warped = cv2.warpPerspective(frame, matrix, (width, height))

        #cv2.imshow("frame", frame)

        clean_line = self.line_detection(warped)
        road, distance, degree = self.fit_polynomial(clean_line)

        Minv = cv2.getPerspectiveTransform(dst_points, src_points)
        road_w = cv2.warpPerspective(road, Minv, (width, height))

        result = cv2.addWeighted(frame, 1, road_w, 1, 0)
        road_on_warped = cv2.addWeighted(warped, 1, road, 1, 0)

        cv2.putText(result, f"Distance : {round(distance, 2)}", (10, 20),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)
        cv2.putText(result, f"Degree : {round(degree, 2)}", (10, 40),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 2)
        
        for pt in src_points.astype(int):
            cv2.circle(result, tuple(pt), 3, color=(0, 0, 255), thickness=-1)

        cv2.imshow("road", road_on_warped)
        cv2.imshow("result", result)
        #cv2.imshow("out_img", road)
        return road, distance, degree


