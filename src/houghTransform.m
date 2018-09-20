clear all
close all
clc

I = imread('../images/png/line.png');
%I  = rgb2gray(I);

BW = edge(I,'canny');
[H,Theta,Rho] = hough(BW);

 figure
 imshow(rescale(H),'XData',Theta,'YData',Rho,...
    'InitialMagnification','fit');
 title('Hough Transform Matrix');
 xlabel('\theta')
 ylabel('\rho');
 axis on, axis normal;
 colormap(gca,hot)