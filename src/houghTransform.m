clear all
close all
clc

I = imread('../images/png/building.png');
I  = rgb2gray(I);

BW = edge(I,'canny');
[H,Theta,Rho] = hough(BW);

 figure
 %imshow(rescale(H),'XData',Theta,'YData',Rho,'InitialMagnification','fit');
 imshow(rescale(H));
 %imshow(H);
 title('Hough Transform Matrix');
 xlabel('\theta')
 ylabel('\rho');
 axis on, axis normal;
 %colormap(gca,hot)